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
#include <mocks/mock_node_base.hpp>
#include <proto/bluzelle.pb.h>
#include <json/json.h>
#include <set>

using namespace ::testing;

namespace {

    const bzn::uuid_t TEST_NODE_UUID{"f0645cc2-476b-485d-b589-217be3ca87d5"};

    const bzn::peers_list_t TEST_PEER_LIST{{"127.0.0.1", 8081, "name1", "uuid1"},
                                           {"127.0.0.1", 8082, "name2", "uuid2"},
                                           {"127.0.0.1", 8084, "name3", TEST_NODE_UUID}};


    class pbft_test : public Test {
    public:
        pbft_msg request_msg;
        pbft_msg preprepare_msg;

        std::shared_ptr<bzn::Mocknode_base> mock_node;
        bzn::pbft pbft;

        pbft_test()
                : mock_node(std::make_shared<bzn::Mocknode_base>())
                , pbft(mock_node, TEST_PEER_LIST, TEST_NODE_UUID)
        {
            request_msg.mutable_request()->set_operation("do some stuff");
            request_msg.mutable_request()->set_client("bob");
            request_msg.mutable_request()->set_timestamp(1);
            request_msg.set_type(PBFT_MSG_TYPE_REQUEST);

            preprepare_msg = pbft_msg(request_msg);
            preprepare_msg.set_type(PBFT_MSG_TYPE_PREPREPARE);
            preprepare_msg.set_sequence(19);
            preprepare_msg.set_view(1);
        }
    };

    pbft_msg extract_pbft_msg(std::shared_ptr<bzn::message> json) {
        pbft_msg result;
        result.ParseFromString((*json)["pbft-data"].asString());
        return result;
    }

    TEST_F(pbft_test, test_requests_create_operations) {
        ASSERT_EQ(0u, pbft.outstanding_operations_count());
        pbft.handle_message(request_msg);
        ASSERT_EQ(1u, pbft.outstanding_operations_count());
    }

    bool is_preprepare(std::shared_ptr<bzn::message> json){
        pbft_msg msg = extract_pbft_msg(json);

        return msg.type() == PBFT_MSG_TYPE_PREPREPARE && msg.view() > 0 && msg.sequence() > 0;
    }

    TEST_F(pbft_test, test_requests_fire_preprepare) {
        EXPECT_CALL(*mock_node, send_message(_, ResultOf(is_preprepare, Eq(true))))
                    .Times(Exactly(TEST_PEER_LIST.size()));

        pbft.handle_message(request_msg);
    }

    std::set<uint64_t> seen_sequences;
    void save_sequences(const boost::asio::ip::tcp::endpoint& /*ep*/, std::shared_ptr<bzn::message> json) {
        pbft_msg msg = extract_pbft_msg(json);
        seen_sequences.insert(msg.sequence());
    }

    TEST_F(pbft_test, test_different_requests_get_different_sequences) {
        EXPECT_CALL(*mock_node, send_message(_, _)).WillRepeatedly(Invoke(save_sequences));

        pbft_msg request_msg2(request_msg);
        request_msg2.mutable_request()->set_timestamp(2);

        seen_sequences = std::set<uint64_t>();
        pbft.handle_message(request_msg);
        pbft.handle_message(request_msg2);
        ASSERT_EQ(seen_sequences.size(), 2u);
    }

    bool is_prepare(std::shared_ptr<bzn::message> json){
        pbft_msg msg = extract_pbft_msg(json);

        return msg.type() == PBFT_MSG_TYPE_PREPARE && msg.view() > 0 && msg.sequence() > 0;
    }

    TEST_F(pbft_test, test_preprepare_triggers_prepare) {
        EXPECT_CALL(*mock_node, send_message(_, ResultOf(is_prepare, Eq(true))))
                .Times(Exactly(TEST_PEER_LIST.size()));

        this->pbft.handle_message(this->preprepare_msg);
    }

    TEST_F(pbft_test, test_prepare_contains_uuid) {
        std::shared_ptr<bzn::message> json;
        EXPECT_CALL(*mock_node, send_message(_, _)).WillRepeatedly(SaveArg<1>(&json));

        this->pbft.handle_message(this->preprepare_msg);

        pbft_msg msg_sent = extract_pbft_msg(json);

        ASSERT_EQ(msg_sent.sender(), this->pbft.get_uuid());
        ASSERT_EQ(msg_sent.sender(), TEST_NODE_UUID);
    }

    TEST_F(pbft_test, test_wrong_view_preprepare_rejected) {
        EXPECT_CALL(*mock_node, send_message(_, _)).Times(Exactly(0));

        pbft_msg preprepare2(this->preprepare_msg);
        preprepare2.set_view(6);

        this->pbft.handle_message(preprepare2);
    }

    TEST_F(pbft_test, test_no_duplicate_prepares_same_sequence_number) {
        EXPECT_CALL(*mock_node, send_message(_, _)).Times(Exactly(TEST_PEER_LIST.size()));

        pbft_msg prepreparea(this->preprepare_msg);
        pbft_msg preprepareb(this->preprepare_msg);
        pbft_msg prepreparec(this->preprepare_msg);
        pbft_msg preprepared(this->preprepare_msg);

        preprepareb.mutable_request()->set_timestamp(99);
        prepreparec.mutable_request()->set_operation("somethign else");
        preprepared.mutable_request()->set_client("certainly not bob");

        this->pbft.handle_message(prepreparea);
        this->pbft.handle_message(preprepareb);
        this->pbft.handle_message(prepreparec);
        this->pbft.handle_message(preprepared);
    }

}
