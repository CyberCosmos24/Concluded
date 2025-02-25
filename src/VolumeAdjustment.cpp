// Copyright 2021 The Mumble Developers. All rights reserved.
// Use of this source code is governed by a BSD-style license
// that can be found in the LICENSE file at the root of the
// Mumble source tree or at <https://www.mumble.info/LICENSE>.

#include "VolumeAdjustment.h"

#include <cassert>
#include <cmath>

constexpr float DB_THRESHOLD = 0.1;

VolumeAdjustment::VolumeAdjustment(float factor, int dbAdjustment) : factor(factor), dbAdjustment(dbAdjustment) {
	assert(dbAdjustment == INVALID_DB_ADJUSTMENT
		   // Verify that the used dbAdjustment is a reasonable representation of the given factor. Particularly, we
		   // want to make sure that the deviation of the true dB representation of the factor (usually a floating point
		   // value) doesn't deviate more than dbThreshold from the passed integer dB value. For all cases, where this
		   // would be the case, we expect INVALID_DB_ADJUSTMENT to be passed instead.
		   //
		   // If dB is the dB-representation of a loudness change factor f, we have
		   // dB = log2(f) * 6    <=>    f = 2^{dB/6}
		   // (+6dB equals a doubling in loudness)
		   || DB_THRESHOLD >= std::abs(dbAdjustment - std::log2(factor) * 6));
}

VolumeAdjustment VolumeAdjustment::fromFactor(float factor) {
	if (factor > 0) {
		float dB = std::log2(factor) * 6;

		if (std::abs(dB - static_cast< int >(dB)) < DB_THRESHOLD) {
			// Close-enough
			return VolumeAdjustment(factor, std::round(dB));
		} else {
			return VolumeAdjustment(factor, INVALID_DB_ADJUSTMENT);
		}
	} else {
		return VolumeAdjustment(factor, INVALID_DB_ADJUSTMENT);
	}
}

VolumeAdjustment VolumeAdjustment::fromDBAdjustment(int dbAdjustment) {
	float factor = std::pow(2.0f, dbAdjustment / 6.0f);

	return VolumeAdjustment(factor, dbAdjustment);
}

bool operator==(const VolumeAdjustment &lhs, const VolumeAdjustment &rhs) {
	return lhs.dbAdjustment == rhs.dbAdjustment && std::abs(lhs.factor - rhs.factor) < 0.1f;
}

bool operator!=(const VolumeAdjustment &lhs, const VolumeAdjustment &rhs) {
	return !(lhs == rhs);
}
