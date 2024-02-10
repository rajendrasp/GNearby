#pragma once

#include <memory>

std::unique_ptr<Frame> DecodeFrame(
    absl::Span<const uint8_t> data) {
    auto frame = std::make_unique<Frame>();

    if (frame->ParseFromArray(data.data(), data.size())) {
        return frame;
    }
    else {
        return nullptr;
    }
}