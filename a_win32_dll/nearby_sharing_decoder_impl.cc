// Copyright 2022 Google LLC
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     https://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

#include "nearby_sharing_decoder_impl.h"

#include <stdint.h>

#include <memory>

#include "absl/types/span.h"
#include "advertisement.h"
#include "sharing/proto/wire_format.pb.h"

namespace nearby {
namespace sharing {

using Frame = ::nearby::sharing::service::proto::Frame;

std::unique_ptr<Advertisement> NearbySharingDecoderImpl::DecodeAdvertisement(
    std::vector<uint8_t> data)
{
    absl::Span<const uint8_t> tmpData(data);
    return Advertisement::FromEndpointInfo(tmpData);
}

std::unique_ptr<Frame> NearbySharingDecoderImpl::DecodeFrame(
    std::vector<uint8_t> data)
{
    absl::Span<const uint8_t> tmpData(data);
    auto frame = std::make_unique<Frame>();

    if (frame->ParseFromArray(tmpData.data(), tmpData.size()))
    {
        return frame;
    }
    else
    {
        return nullptr;
    }
}

}  // namespace sharing
}  // namespace nearby
