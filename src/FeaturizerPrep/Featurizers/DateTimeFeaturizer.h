// ----------------------------------------------------------------------
// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License
// ----------------------------------------------------------------------
#pragma once

#include <chrono>
#include <ctime>
#include <cstdint>
#include <stdexcept>

#include "../InferenceOnlyFeaturizerImpl.h"

namespace Microsoft {
namespace Featurizer {

/////////////////////////////////////////////////////////////////////////
///  \struct        TimePoint
///  \brief         Struct to hold various components of DateTime information
///
struct TimePoint {
    std::int32_t year = 0;
    std::uint8_t month = 0;         /* 1-12 */
    std::uint8_t day = 0;           /* 1-31 */
    std::uint8_t hour = 0;          /* 0-23 */
    std::uint8_t minute = 0;        /* 0-59 */
    std::uint8_t second = 0;        /* 0-59 */
    std::uint8_t dayOfWeek = 0;     /* 0-6 */
    std::uint16_t dayOfYear = 0;    /* 0-365 */
    std::uint8_t quarterOfYear = 0; /* 1-4 */
    std::uint8_t weekOfMonth = 0;   /* 0-4 */

    TimePoint(const std::chrono::system_clock::time_point& sysTime);

    TimePoint(TimePoint const &) = delete;
    TimePoint & operator =(TimePoint const &) = delete;

    TimePoint(TimePoint &&) = default;
    TimePoint & operator =(TimePoint &&) = delete;

    enum {
        JANUARY = 1, FEBRUARY, MARCH, APRIL, MAY, JUNE,
        JULY, AUGUST, SEPTEMBER, OCTOBER, NOVEMBER, DECEMBER
    };
    enum {
        SUNDAY = 0, MONDAY, TUESDAY, WEDNESDAY, THURSDAY, FRIDAY, SATURDAY
    };
};

/////////////////////////////////////////////////////////////////////////
///  \class         DateTimeTransformer
///  \brief         A Transformer that takes a chrono::system_clock::time_point and
///                 returns a struct with all the data split out.
///
class DateTimeTransformer : public InferenceOnlyTransformerImpl<std::chrono::system_clock::time_point, TimePoint> {
public:
    // ----------------------------------------------------------------------
    // |
    // |  Public Types
    // |
    // ----------------------------------------------------------------------
    using BaseType                          = InferenceOnlyTransformerImpl<std::chrono::system_clock::time_point, TimePoint>;

    // ----------------------------------------------------------------------
    // |
    // |  Public Methods
    // |
    // ----------------------------------------------------------------------
    DateTimeTransformer(void) = default;
    DateTimeTransformer(Archive &ar);

    ~DateTimeTransformer(void) override = default;

    DateTimeTransformer(DateTimeTransformer const &) = delete;
    DateTimeTransformer & operator =(DateTimeTransformer const &) = delete;

    DateTimeTransformer(DateTimeTransformer &&) = default;
    DateTimeTransformer & operator =(DateTimeTransformer &&) = delete;

    TransformedType execute(InputType input) override;
};

class DateTimeEstimator : public InferenceOnlyEstimatorImpl<DateTimeTransformer> {
public:
    // ----------------------------------------------------------------------
    // |
    // |  Public Types
    // |
    // ----------------------------------------------------------------------
    using BaseType                          = InferenceOnlyEstimatorImpl<DateTimeTransformer>;

    // ----------------------------------------------------------------------
    // |
    // |  Public Methods
    // |
    // ----------------------------------------------------------------------
    DateTimeEstimator(AnnotationMapsPtr pAllColumnAnnotations);
    ~DateTimeEstimator(void) override = default;

    DateTimeEstimator(DateTimeEstimator const &) = delete;
    DateTimeEstimator & operator =(DateTimeEstimator const &) = delete;

    DateTimeEstimator(DateTimeEstimator &&) = default;
    DateTimeEstimator & operator =(DateTimeEstimator &&) = delete;
};

} // Namespace Featurizer
} // Namespace Microsoft
