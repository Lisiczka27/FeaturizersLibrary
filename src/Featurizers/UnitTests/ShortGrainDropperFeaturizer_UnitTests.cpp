// ----------------------------------------------------------------------
// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License
// ----------------------------------------------------------------------
#define CATCH_CONFIG_MAIN
#include "catch.hpp"

#include "../../Featurizers/ShortGrainDropperFeaturizer.h"
#include "../TestHelpers.h"
#include "../../Archive.h"

namespace NS = Microsoft::Featurizer;

void TestImpl(){
    std::vector<std::vector<std::vector<std::string>>> trainingBatches = 	NS::TestHelpers::make_vector<std::vector<std::vector<std::string>>>(
        NS::TestHelpers::make_vector<std::vector<std::string>>(
            NS::TestHelpers::make_vector<std::string>("a", "b"),
            NS::TestHelpers::make_vector<std::string>("a", "b"),
            NS::TestHelpers::make_vector<std::string>("a", "b"),
            NS::TestHelpers::make_vector<std::string>("a", "b"),
            NS::TestHelpers::make_vector<std::string>("a", "b"),
            NS::TestHelpers::make_vector<std::string>("a", "c"),
            NS::TestHelpers::make_vector<std::string>("a", "c"),
            NS::TestHelpers::make_vector<std::string>("a", "c"),
            NS::TestHelpers::make_vector<std::string>("a", "c"),
            NS::TestHelpers::make_vector<std::string>("a", "d"),
            NS::TestHelpers::make_vector<std::string>("a", "d"),
            NS::TestHelpers::make_vector<std::string>("a", "d"),
            NS::TestHelpers::make_vector<std::string>("c", "d")
        )
    );

    std::vector<std::vector<std::string>> inferencingInput = NS::TestHelpers::make_vector<std::vector<std::string>>(
        NS::TestHelpers::make_vector<std::string>("a", "b"),
        NS::TestHelpers::make_vector<std::string>("a", "c"),
        NS::TestHelpers::make_vector<std::string>("a", "d"),
        NS::TestHelpers::make_vector<std::string>("c", "d"),
        NS::TestHelpers::make_vector<std::string>("e", "f")
    );

    std::vector<bool> inferencingOutput = {
        false,
        true,
        true,
        true,
        false
    };

    //parameter setting
    std::uint32_t windowSize = 0;
    std::vector<std::uint32_t> lags = NS::TestHelpers::make_vector<std::uint32_t>(0, 0);
    std::uint32_t maxHorizon = 1;
    nonstd::optional<std::uint32_t> cv = static_cast<std::uint32_t>(1);

    using SGDEstimator = NS::Featurizers::ShortGrainDropperEstimator<std::numeric_limits<size_t>::max()>;
    SGDEstimator                     estimator(NS::CreateTestAnnotationMapsPtr(1), 0, windowSize, lags, maxHorizon, cv);

    NS::TestHelpers::Train<SGDEstimator, std::vector<std::string>>(estimator, trainingBatches);
    SGDEstimator::TransformerUniquePtr                  pTransformer(estimator.create_transformer());
    std::vector<bool>   	                            output;

    auto const              callback(
        [&output](bool value) {
            output.push_back(value);
        }
    );

    for(auto const &item : inferencingInput)
        pTransformer->execute(item, callback);

    pTransformer->flush(callback);

    CHECK(output == inferencingOutput);
}

TEST_CASE("Invalid Transformer/Estimator") {
    //parameter setting
    std::uint32_t windowSize = 0;
    std::vector<std::uint32_t> lags = NS::TestHelpers::make_vector<std::uint32_t>();
    std::uint32_t maxHorizon = 1;
    nonstd::optional<std::uint32_t> cv = static_cast<std::uint32_t>(1);

    CHECK_THROWS_WITH(NS::Featurizers::ShortGrainDropperEstimator<std::numeric_limits<size_t>::max()>(NS::CreateTestAnnotationMapsPtr(1), 2, windowSize, lags, maxHorizon, cv), "colIndex");
    CHECK_THROWS_WITH(NS::Featurizers::ShortGrainDropperEstimator<std::numeric_limits<size_t>::max()>(NS::CreateTestAnnotationMapsPtr(1), 0, windowSize, lags, maxHorizon, cv), "lags");
}

TEST_CASE("Standard Test") {
    TestImpl();
}

TEST_CASE("Serialization/Deserialization") {
    using TransformerType = NS::Featurizers::ShortGrainDropperTransformer;

    std::unordered_set<std::string>         grainsToDrop({"aa", "ab"});

    TransformerType                         original(std::move(grainsToDrop));
    NS::Archive                             out;

    original.save(out);

    NS::Archive                             in(out.commit());
    TransformerType                         other(in);

    CHECK(other == original);
}

TEST_CASE("Serialization Version Error") {
    NS::Archive                             out;

    out.serialize(static_cast<std::uint16_t>(2));
    out.serialize(static_cast<std::uint16_t>(0));

    NS::Archive                             in(out.commit());

    CHECK_THROWS_WITH(
        NS::Featurizers::ShortGrainDropperTransformer(in),
        Catch::Contains("Unsupported archive version")
    );
}
