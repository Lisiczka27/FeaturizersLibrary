// ----------------------------------------------------------------------
// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License
// ----------------------------------------------------------------------
#pragma once

#include <queue>

#include "TrainingOnlyEstimatorImpl.h"

namespace Microsoft {
namespace Featurizer {
namespace Featurizers {
namespace Components {

static constexpr char const * const         MedianEstimatorName("MedianEstimator");

/////////////////////////////////////////////////////////////////////////
///  \class         MedianAnnotationData
///  \brief         Annotation produced that contains the median value encountered
///                 during training.
///
template <typename T>
class MedianAnnotationData {
public:
    // ----------------------------------------------------------------------
    // |
    // |  Public Data
    // |
    // ----------------------------------------------------------------------
    T const                                 Median;

    // ----------------------------------------------------------------------
    // |
    // |  Public Methods
    // |
    // ----------------------------------------------------------------------
    MedianAnnotationData(T median);
    ~MedianAnnotationData(void) = default;

    FEATURIZER_MOVE_CONSTRUCTOR_ONLY(MedianAnnotationData);
};

namespace Details {

/////////////////////////////////////////////////////////////////////////
///  \class         MedianTrainingOnlyPolicy
///  \brief         `MedianEstimator` implementation details.
///
template <typename InputT, typename TransformedT, bool InterpolateValuesV>
class MedianTrainingOnlyPolicy {
public:
    // ----------------------------------------------------------------------
    // |
    // |  Public Types
    // |
    // ----------------------------------------------------------------------
    static_assert(Traits<TransformedT>::IsNullableType == false || Traits<TransformedT>::IsNativeNullableType, "'TransformedT' should not be a nullable type");

    using InputType                         = InputT;

    // ----------------------------------------------------------------------
    // |
    // |  Public Data
    // |
    // ----------------------------------------------------------------------
    static constexpr char const * const     NameValue = MedianEstimatorName;

    // ----------------------------------------------------------------------
    // |
    // |  Public Methods
    // |
    // ----------------------------------------------------------------------
    void fit(InputType const &input);
    MedianAnnotationData<TransformedT> complete_training(void);

private:
    // ----------------------------------------------------------------------
    // |
    // |  Private Types
    // |
    // ----------------------------------------------------------------------
    // Note that a priority_queue is used here over another type, as a priority_queue
    // ensures that sorting only happens when a value is retrieved while sorting would
    // need to happen during insertion for other types. For this algorithm, it means
    // that sorting only happens when we need to rebalance the queues.
    using MaxHeapType                       = std::priority_queue<TransformedT>;
    using MinHeapType                       = std::priority_queue<TransformedT, std::vector<TransformedT>, std::greater<TransformedT>>;

    // ----------------------------------------------------------------------
    // |
    // |  Private Data
    // |
    // ----------------------------------------------------------------------
    MaxHeapType                             _smaller;
    MinHeapType                             _larger;

    // ----------------------------------------------------------------------
    // |
    // |  Private Methods
    // |
    // ----------------------------------------------------------------------
    void fit_impl(InputType const &input, std::true_type /*is_nullable*/);
    void fit_impl(InputType const &input, std::false_type /*is_nullable*/);

    template <typename U>
    void fit_impl(U const &input);

    TransformedT _get_interpolated_value(std::true_type /*supports Interpolated values*/);
    TransformedT _get_interpolated_value(std::false_type /*supports Interpolated values*/);
};

} // namespace Details

/////////////////////////////////////////////////////////////////////////
///  \typedef       MedianEstimator
///  \brief         An `Estimator` that computes the median value encountered
///                 during training.
///
template <
    typename InputT,
    typename TransformedT=InputT,
    bool InterpolateValuesV=true,
    size_t MaxNumTrainingItemsV=std::numeric_limits<size_t>::max()
>
using MedianEstimator                       = TrainingOnlyEstimatorImpl<Details::MedianTrainingOnlyPolicy<InputT, TransformedT, InterpolateValuesV>, MaxNumTrainingItemsV>;

// ----------------------------------------------------------------------
// ----------------------------------------------------------------------
// ----------------------------------------------------------------------
// |
// |  Implementation
// |
// ----------------------------------------------------------------------
// ----------------------------------------------------------------------
// ----------------------------------------------------------------------

// ----------------------------------------------------------------------
// |
// |  MedianAnnotationData
// |
// ----------------------------------------------------------------------
template <typename T>
MedianAnnotationData<T>::MedianAnnotationData(T median) :
    Median(std::move(median)) {
}

// ----------------------------------------------------------------------
// |
// |  Details::MedianTrainingOnlyPolicy
// |
// ----------------------------------------------------------------------
template <typename InputT, typename TransformedT, bool InterpolateValuesV>
void Details::MedianTrainingOnlyPolicy<InputT, TransformedT, InterpolateValuesV>::fit(InputType const &input) {
    fit_impl(input, std::integral_constant<bool, Traits<InputT>::IsNullableType>());
}

template <typename InputT, typename TransformedT, bool InterpolateValuesV>
MedianAnnotationData<TransformedT> Details::MedianTrainingOnlyPolicy<InputT, TransformedT, InterpolateValuesV>::complete_training(void) {
    size_t const                            numElements(_smaller.size() + _larger.size());

    if(numElements == 0)
        throw std::runtime_error("No elements were provided during training");

    TransformedT                            median;

#if (defined _MSC_VER)
#   pragma warning(push)
#   pragma warning(disable: 4127) // conditional expression is constant
#endif

    if(InterpolateValuesV == false || numElements & 1) {
        assert(_smaller.empty() == false);
        assert(InterpolateValuesV == false || _smaller.size() == _larger.size() + 1);

        median = _smaller.top();
    }
    else
        median = this->_get_interpolated_value(std::integral_constant<bool, InterpolateValuesV>());

#if (defined _MSC_VER)
#   pragma warning(pop)
#endif

    // Clean up after ourselves
    _smaller = MaxHeapType();
    _larger = MinHeapType();

    return median;
}

// ----------------------------------------------------------------------
// ----------------------------------------------------------------------
// ----------------------------------------------------------------------
template <typename InputT, typename TransformedT, bool InterpolateValuesV>
void Details::MedianTrainingOnlyPolicy<InputT, TransformedT, InterpolateValuesV>::fit_impl(InputType const &input, std::true_type /*is_nullable*/) {
    if(Traits<InputT>::IsNull(input))
        return;

    fit_impl(Traits<InputT>::GetNullableValue(input));
}

template <typename InputT, typename TransformedT, bool InterpolateValuesV>
void Details::MedianTrainingOnlyPolicy<InputT, TransformedT, InterpolateValuesV>::fit_impl(InputType const &input, std::false_type /*is_nullable*/) {
    fit_impl(input);
}

template <typename InputT, typename TransformedT, bool InterpolateValuesV>
template <typename U>
void Details::MedianTrainingOnlyPolicy<InputT, TransformedT, InterpolateValuesV>::fit_impl(U const &input) {
    if(_smaller.empty() || static_cast<TransformedT>(input) <= _smaller.top()) {
#if (defined __apple_build_version__ || defined __GNUC__ && (__GNUC__ < 4 || (__GNUC__ == 4 && __GNUC_MINOR__ <= 8)))
        _smaller.push(static_cast<TransformedT>(input));
#else
        _smaller.emplace(static_cast<TransformedT>(input));
#endif
    }
    else {
#if (defined __apple_build_version__ || defined __GNUC__ && (__GNUC__ < 4 || (__GNUC__ == 4 && __GNUC_MINOR__ <= 8)))
        _larger.push(static_cast<TransformedT>(input));
#else
        _larger.emplace(static_cast<TransformedT>(input));
#endif
    }

    // Rebalance if necessary
    if(_smaller.size() >= _larger.size() + 2) {
#if (defined __apple_build_version__ || defined __GNUC__ && (__GNUC__ < 4 || (__GNUC__ == 4 && __GNUC_MINOR__ <= 8)))
        _larger.push(_smaller.top());
#else
        _larger.emplace(_smaller.top());
#endif
        _smaller.pop();
    }
    else if(_larger.size() > _smaller.size()) {
#if (defined __apple_build_version__ || defined __GNUC__ && (__GNUC__ < 4 || (__GNUC__ == 4 && __GNUC_MINOR__ <= 8)))
        _smaller.push(_larger.top());
#else
        _smaller.emplace(_larger.top());
#endif
        _larger.pop();
    }
}

template <typename InputT, typename TransformedT, bool InterpolateValuesV>
TransformedT Details::MedianTrainingOnlyPolicy<InputT, TransformedT, InterpolateValuesV>::_get_interpolated_value(std::true_type /*supports Interpolated values*/) {
    assert(_smaller.empty() == false);

    TransformedT const                      greater(_larger.empty() ? 0 : _larger.top());

#if (defined __clang__)
#   pragma clang diagnostic push
#   pragma clang diagnostic ignored "-Wdouble-promotion"
#endif

    return static_cast<TransformedT>((static_cast<double>(_smaller.top()) + greater) / 2);

#if (defined __clang__)
#   pragma clang diagnostic pop
#endif
}

template <typename InputT, typename TransformedT, bool InterpolateValuesV>
TransformedT Details::MedianTrainingOnlyPolicy<InputT, TransformedT, InterpolateValuesV>::_get_interpolated_value(std::false_type /*supports Interpolated values*/) {
    throw std::runtime_error("This should never be called");
}

} // namespace Components
} // namespace Featurizers
} // namespace Featurizer
} // namespace Microsoft
