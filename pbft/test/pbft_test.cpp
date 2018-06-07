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
#include <pbft/pbft_base.hpp>
#include <pbft/pbft.hpp>
#include <bootstrap/bootstrap_peers.hpp>
#include <proto/bluzelle.pb.h>

using namespace ::testing;

namespace {

    const bzn::uuid_t TEST_NODE_UUID{"f0645cc2-476b-485d-b589-217be3ca87d5"};

    const bzn::peers_list_t TEST_PEER_LIST{{"127.0.0.1", 8081, "name1", "uuid1"},
                                           {"127.0.0.1", 8082, "name2", "uuid2"},
                                           {"127.0.0.1", 8084, "name3", TEST_NODE_UUID}};


    class pbft_test : public Test {
    public:
        bzn::pbft pbft;
        pbft_msg msg;
        pbft_request req;

        pbft_test()
                : pbft(nullptr, TEST_PEER_LIST)
        {
            msg.set_allocated_request(&req);
        }
    };

    TEST_F(pbft_test, test_requests_create_operations) {
        ASSERT_EQ(0u, pbft.outstanding_operations_count());
        pbft.handle_message(msg);
        ASSERT_EQ(1u, pbft.outstanding_operations_count());
    }

    TEST_F(pbft_test, test_requests_dont_create_duplicate_operations_count) {
        ASSERT_EQ(0u, pbft.outstanding_operations_count());
        pbft.handle_message(msg);
        pbft.handle_message(msg);
        ASSERT_EQ(1u, pbft.outstanding_operations_count());

    }
}
