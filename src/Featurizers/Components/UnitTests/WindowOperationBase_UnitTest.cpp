// ----------------------------------------------------------------------
// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License
// ----------------------------------------------------------------------
#define CATCH_CONFIG_MAIN
#include "catch.hpp"

#include "../../../3rdParty/optional.h"
#include "../WindowOperationBase.h"

namespace NS = Microsoft::Featurizer;


TEST_CASE("CircularIterator - Simple Test") {
    // 20 is longer than the input array. Testing looping capabilities.
    std::vector<std::int16_t> v{1,2,3,4,5};
    NS::Featurizers::Components::CircularIterator<std::int16_t> iter(&v[0], v.size(), 20);
    CHECK(*iter == 1);
    iter++;
    CHECK(*iter == 2);
    iter++;
    CHECK(*iter == 3);
    iter++;
    CHECK(*iter == 4);
    iter++;
    CHECK(*iter == 5);
    iter++;
    CHECK(*iter == 1);
    iter++;
    CHECK(*iter == 2);
    iter++;
    CHECK(*iter == 3);
    iter++;
    CHECK(*iter == 4);
    iter++;
    CHECK(*iter == 5);
    iter++;
}

TEST_CASE("CircularIterator - Comparison Test") {
    // 20 is longer than the input array. Testing looping capabilities while comparing iterators.
    std::vector<std::int16_t> v{1,2,3,4,5};
    NS::Featurizers::Components::CircularIterator<std::int16_t> iter1(&v[0], v.size(), 20);
    NS::Featurizers::Components::CircularIterator<std::int16_t> iter2(&v[0], v.size(), 20);

    // Should start out being equal
    CHECK(iter1 == iter2);

    // After update shouldn't be equal
    iter2++;
    CHECK(iter1 != iter2);

    // Should be equal again
    iter1++;
    CHECK(iter1 == iter2);

    // Doing a full loop should result in the iterators being equal
    // iter1 = 3, iter2 = 2
    CHECK(++iter1 != iter2);

    // iter1 = 4, iter2 = 2
    CHECK(++iter1 != iter2);

    // iter1 = 5, iter2 = 2
    CHECK(++iter1 != iter2);

    // iter1 = 1, iter2 = 2
    CHECK(++iter1 != iter2);

    // iter1 = 2, iter2 = 2
    CHECK(++iter1 == iter2);
}

TEST_CASE("CircularIterator - Operations Test") {
    std::vector<std::int16_t> v{1,2,3,4,5};

    NS::Featurizers::Components::CircularIterator<std::int16_t> iter(&v[0], v.size(), 5);

    // Start deference should equal one
    CHECK(*iter == 1);

    // Post increment check
    CHECK(*iter++ == 1);
    CHECK(*iter == 2);

    // Pre increment check
    CHECK(*++iter == 3);
}

TEST_CASE("CircularIterator - String Test") {
    std::vector<std::string> v{"1","2","3","4","5"};

    NS::Featurizers::Components::CircularIterator<std::string> iter(&v[0], v.size(), 5);

    // Start deference should equal one
    CHECK(*iter == "1");

    // Post increment check
    CHECK(*iter++ == "1");
    CHECK(*iter == "2");

    // Pre increment check
    CHECK(*++iter == "3");

    // Check -> operator
    CHECK(iter->length() == 1);
}

TEST_CASE("CircularIterator - Array Test") {
    // 20 is longer than the input array. Testing looping capabilities. Just validating it works with multiple containers.
    std::int16_t v[]{1,2,3,4,5};

    NS::Featurizers::Components::CircularIterator<std::int16_t> iter1(&v[0], 5, 20);
    NS::Featurizers::Components::CircularIterator<std::int16_t> iter2(&v[0], 5, 20);

    // Should start out being equal
    CHECK(iter1 == iter2);

    // After update shouldn't be equal
    iter2++;
    CHECK(iter1 != iter2);

    // Should be equal again
    iter1++;
    CHECK(iter1 == iter2);

    // Doing a full loop should result in the iterators being equal
    // iter1 = 3, iter2 = 2
    CHECK(++iter1 != iter2);

    // iter1 = 4, iter2 = 2
    CHECK(++iter1 != iter2);

    // iter1 = 5, iter2 = 2
    CHECK(++iter1 != iter2);

    // iter1 = 1, iter2 = 2
    CHECK(++iter1 != iter2);

    // iter1 = 2, iter2 = 2
    CHECK(++iter1 == iter2);
}

TEST_CASE("CircularIterator - Single Value Test") {
    std::int16_t v = 1;

    NS::Featurizers::Components::CircularIterator<std::int16_t> iter1(&v, 1, 2);
    NS::Featurizers::Components::CircularIterator<std::int16_t> iter2(&v, 1, 2);

    // Should start out being equal
    CHECK(iter1 == iter2);

    // Since only 1 value, increment should keep the same value
    CHECK(++iter1 == iter2);

    // Since only 1 value, increment should keep the same value
    CHECK(iter1 == ++iter2);

}

TEST_CASE("CircularIterator - Starting offset Test") {
    std::vector<std::int16_t> v{1,2,3,4,5};

    NS::Featurizers::Components::CircularIterator<std::int16_t> iter1(&v[0], v.size(), 5, 2);
    NS::Featurizers::Components::CircularIterator<std::int16_t> iter2(&v[0], v.size(), 5);

    // Should NOT start out being equal since iter1 has an offset of 2
    CHECK(iter1 != iter2);

    CHECK(*iter1 == 3);

    // Increment iter2 twice to equal iter1
    CHECK(iter1 == ++(++iter2));

}

TEST_CASE("CircularIterator - End iterator Test") {
    std::vector<std::int16_t> v{1,2,3,4,5};

    NS::Featurizers::Components::CircularIterator<std::int16_t> iter1(&v[0], v.size(), 1, 2);
    NS::Featurizers::Components::CircularIterator<std::int16_t> end_iter(&v[0], v.size(), 0, 3);

    // Should NOT start out being equal since iter1 is not at end but end_iter is.
    CHECK(iter1 != end_iter);

    CHECK(*iter1 == 3);

    // Increment iter1 to equal end_iter
    CHECK(++iter1 == end_iter);

}

TEST_CASE("CircularIterator - End iterator Test While Loop") {
    std::vector<std::int16_t> v{1,2,3,4,5};

    NS::Featurizers::Components::CircularIterator<std::int16_t> iter1(&v[0], v.size(), v.size());
    NS::Featurizers::Components::CircularIterator<std::int16_t> end_iter(&v[0], v.size(), 0);

    // Should NOT start out being equal since iter1 is not at end but end_iter is.
    CHECK(iter1 != end_iter);

    // Loop till iter1 equals end_iter. Should loop 5 times.
    size_t count = 0;
    while(iter1 != end_iter) {
        ++iter1;
        ++count;
    }

    // Make sure it looped the correct number of times.
    CHECK(count == 5);

    // After while loop iterators should be the same
    CHECK(iter1 == end_iter);
}

TEST_CASE("CircularIterator - Const check") {
    std::vector<std::int16_t> v{1,2,3,4,5};

    NS::Featurizers::Components::CircularIterator<const std::int16_t> iter1(&v[0], v.size(), v.size());
    NS::Featurizers::Components::CircularIterator<const std::int16_t> end_iter(&v[0], v.size(), 0);

    // Should NOT start out being equal since iter1 is not at end but end_iter is.
    CHECK(iter1 != end_iter);

    // Loop till iter1 equals end_iter. Should loop 5 times.
    size_t count = 0;
    while(iter1 != end_iter) {
        ++iter1;
        ++count;
    }

    // Make sure it looped the correct number of times.
    CHECK(count == 5);

    // After while loop iterators should be the same
    CHECK(iter1 == end_iter);
}

TEST_CASE("CircularIterator - Default constructor") {
    NS::Featurizers::Components::CircularIterator<std::string> iter1;
    NS::Featurizers::Components::CircularIterator<std::string> iter2;

    // Make sure 2 iterators that were default constructed compare as equal
    CHECK(iter1 == iter2);
}

TEST_CASE("CircularBuffer - Empty buffer") {
    NS::Featurizers::Components::CircularBuffer<std::string> circ_buf(5);

    auto start_iter = circ_buf.begin();
    auto end_iter = circ_buf.end();

    // Since there is no data, the start and end iterators should be equal.
    CHECK(start_iter == end_iter);
}

TEST_CASE("CircularBuffer - Push") {
    NS::Featurizers::Components::CircularBuffer<std::string> circ_buf(5);

    circ_buf.push("1");

    auto start_iter = circ_buf.begin();
    auto end_iter = circ_buf.end();

    // Since there is data, the start and end iterators shouldn't be equal.
    CHECK(start_iter != end_iter);

    // Make sure the value is what we expect
    CHECK(*start_iter == "1");

    // Make sure after 1 increment start and end are equal
    CHECK(++start_iter == end_iter);
}

TEST_CASE("CircularBuffer - Circular Push") {
    NS::Featurizers::Components::CircularBuffer<std::string> circ_buf(5);

    circ_buf.push("1");
    circ_buf.push("2");
    circ_buf.push("3");
    circ_buf.push("4");
    circ_buf.push("5");
    circ_buf.push("6");

    auto start_iter = circ_buf.begin();
    auto end_iter = circ_buf.end();

    // Make sure after fully populated, new pushes will change the begin()
    CHECK(*start_iter == "2");

    size_t count = 0;
    auto it1 = start_iter;
    while(it1 != end_iter) {
        ++it1;
        ++count;
    }
    // Make sure it looped the correct number of times.
    CHECK(count == 5);

    auto it2 = start_iter;
    for (size_t i=0; i<4; i++) {
        ++it2;
    }

    // Make sure new value replaces
    CHECK(*it2 == "6");
}

TEST_CASE("CircularBuffer - Range") {
    using iterator = typename NS::Featurizers::Components::CircularBuffer<std::int16_t>::iterator;
    NS::Featurizers::Components::CircularBuffer<std::int16_t> circ_buf(5);

    circ_buf.push(1);
    circ_buf.push(2);
    circ_buf.push(3);

    // When the vector is not fully populated and the number of requested items is less than the current populated size
    std::tuple<iterator, iterator> range = circ_buf.range(2);

    auto start_iter = std::get<0>(range);
    auto end_iter = std::get<1>(range);

    size_t count = 0;
    while(start_iter != end_iter) {
        ++start_iter;
        ++count;
    }
    CHECK(count == 2);



    // When the vector is not fully populated and the number of requested items is greater than the current populated size
    // the end iterator should be bounded by the currect populated size
    range = circ_buf.range(4);

    start_iter = std::get<0>(range);
    end_iter = std::get<1>(range);

    count = 0;
    while(start_iter != end_iter) {
        ++start_iter;
        ++count;
    }
    CHECK(count == 3);
}
